----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/22/2025 10:31:54 PM
-- Design Name: 
-- Module Name: mux2_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux2_tb is
--  Port ( );
end mux2_tb;

architecture Behavioral of mux2_tb is

component mux2_struct  
    Port ( A : in STD_LOGIC;
       B : in STD_LOGIC;
       S : in STD_LOGIC;
       Q : out STD_LOGIC);
end component;

component mux2_beh  
    Port ( A : in STD_LOGIC;
       B : in STD_LOGIC;
       S : in STD_LOGIC;
       Q : out STD_LOGIC);
end component;

signal test_vector : std_logic_vector (2 downto 0);

signal a_int : std_logic;
signal b_int : std_logic;
signal s_int : std_logic;


signal q_beh : std_logic;
signal q_struct : std_logic;

signal error : std_logic;

constant period : time := 25 ns;

begin

    mux_beh: mux2_beh port map(
        a => a_int,
        b => b_int,
        s => s_int,
        q => q_beh
     );
     
     mux_struct: mux2_struct port map(
        a => a_int,
         b => b_int,
         s => s_int,
        q => q_struct
     );
     
     a_int <= test_vector(2);
     b_int <= test_vector(1);
     s_int <= test_vector(0);
     
     error <= q_struct xor q_beh;
     
     tb_proc : process
         variable error_count : integer := 0;
      begin
         
         report "Starting testbench...";
         
         for i in 0 to 7 loop
             test_vector <= std_logic_vector(to_unsigned(i, test_vector'length));
             wait for period;
             
             if error = '1' then
                 error_count := error_count + 1;
                 report "ERROR at test vector: " & integer'image(i);
             end if;
         end loop;
         
         if error_count = 0 then
             report "ALL TESTS PASSED! Both implementations are equivalent." severity note;
         else
             report "TEST FAILED: "& integer'image(error_count) severity note;
         end if;
         
         report "End of simulation" severity failure;
      
      end process;

end Behavioral;
